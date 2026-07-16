//
//  GLView.mm
//  LearnOpenGL
//

#import "GLView.h"

#import <OpenGL/gl3.h>
#import <CoreVideo/CoreVideo.h>

#include <memory>

#include "DualViewportRenderer.hpp"
#include "Exercise.hpp"
#include "ExerciseRegistry.hpp"

// NSOpenGLView / NSOpenGLContext 在新版 macOS 已弃用但仍可用（OpenGL 上限 4.1）。
// 这是学习 OpenGL 的唯一原生路径，主动静默弃用告警。
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

namespace {

double hostSeconds() {
    return (double)CVGetCurrentHostTime() / (double)CVGetHostClockFrequency();
}

} // namespace

@implementation GLView {
    CVDisplayLinkRef _displayLink;
    GLsizei _vpWidth;   // 像素单位（已乘 backing scale），仅在主线程写、渲染线程读
    GLsizei _vpHeight;

    std::unique_ptr<DualViewportRenderer> _renderer;
    std::unique_ptr<Exercise> _exercise;
    double _startTime;
    double _lastTime;
}

- (instancetype)initWithFrame:(NSRect)frameRect {
    NSOpenGLPixelFormatAttribute attrs[] = {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
        NSOpenGLPFAColorSize,     24,
        NSOpenGLPFAAlphaSize,     8,
        NSOpenGLPFADepthSize,     24,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,
        0
    };
    NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    if (!pixelFormat) {
        NSLog(@"[GLView] 无法创建 OpenGL 4.1 Core Profile 像素格式");
    }

    self = [super initWithFrame:frameRect pixelFormat:pixelFormat];
    if (self) {
        self.wantsBestResolutionOpenGLSurface = YES; // retina 原生分辨率
    }
    return self;
}

#pragma mark - 上下文初始化与渲染循环

- (void)prepareOpenGL {
    [super prepareOpenGL];

    GLint swapInterval = 1; // 垂直同步
    [self.openGLContext setValues:&swapInterval
                     forParameter:NSOpenGLContextParameterSwapInterval];

    [self.openGLContext makeCurrentContext];
    NSLog(@"[GLView] GL_VERSION  = %s", glGetString(GL_VERSION));
    NSLog(@"[GLView] GL_RENDERER = %s", glGetString(GL_RENDERER));
    NSLog(@"[GLView] GLSL        = %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glEnable(GL_DEPTH_TEST);

    _renderer = std::make_unique<DualViewportRenderer>();
    _renderer->setup();

    [self switchToExerciseAtIndex:0]; // 默认加载第一个练习（Hello Triangle）

    [self updateBackingSize];

    CVDisplayLinkCreateWithActiveCGDisplays(&_displayLink);
    CVDisplayLinkSetOutputCallback(_displayLink, &GLViewDisplayLinkCallback, (__bridge void *)self);
    CGLContextObj cglContext = self.openGLContext.CGLContextObj;
    CGLPixelFormatObj cglPixelFormat = self.pixelFormat.CGLPixelFormatObj;
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(_displayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(_displayLink);
}

// CVDisplayLink 回调运行在独立线程
static CVReturn GLViewDisplayLinkCallback(CVDisplayLinkRef displayLink,
                                          const CVTimeStamp *now,
                                          const CVTimeStamp *outputTime,
                                          CVOptionFlags flagsIn,
                                          CVOptionFlags *flagsOut,
                                          void *context) {
    @autoreleasepool {
        GLView *view = (__bridge GLView *)context;
        [view renderFrame];
    }
    return kCVReturnSuccess;
}

- (void)renderFrame {
    NSOpenGLContext *context = self.openGLContext;
    if (!context || !_renderer || !_exercise) {
        return;
    }

    CGLLockContext(context.CGLContextObj);
    [context makeCurrentContext];

    const double now = hostSeconds();
    const float time = (float)(now - _startTime);
    const float dt = (float)(now - _lastTime);
    _lastTime = now;

    _exercise->update(dt, time);
    _renderer->update(dt);
    _renderer->render(_vpWidth, _vpHeight, *_exercise);

    [context flushBuffer];
    CGLUnlockContext(context.CGLContextObj);
}

#pragma mark - 练习切换

- (NSArray<NSString *> *)exerciseNames {
    const auto& registry = exerciseRegistry();
    NSMutableArray<NSString *> *names = [NSMutableArray arrayWithCapacity:registry.size()];
    for (const auto& entry : registry) {
        [names addObject:[NSString stringWithUTF8String:entry.name.c_str()]];
    }
    return names;
}

- (void)switchToExerciseAtIndex:(NSInteger)index {
    const auto& registry = exerciseRegistry();
    if (index < 0 || index >= (NSInteger)registry.size()) {
        return;
    }

    CGLLockContext(self.openGLContext.CGLContextObj);
    [self.openGLContext makeCurrentContext];
    _exercise = registry[(size_t)index].create(); // 旧练习在此析构（上下文已 current）
    _exercise->setup();
    CGLUnlockContext(self.openGLContext.CGLContextObj);

    _startTime = hostSeconds();
    _lastTime = _startTime;
    NSLog(@"[GLView] 练习 #%ld: %s", (long)(index + 1), registry[(size_t)index].name.c_str());

    if (self.exerciseDidChange) {
        self.exerciseDidChange(index); // 通知外部 UI 同步选中项
    }
}

#pragma mark - 输入：驱动观察者相机 / 切换练习

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)viewDidMoveToWindow {
    [super viewDidMoveToWindow];
    [self.window makeFirstResponder:self]; // 接收键盘事件以切换练习
}

- (void)mouseDragged:(NSEvent *)event {
    if (_renderer) {
        _renderer->onDrag((float)event.deltaX, (float)event.deltaY);
    }
}

- (void)scrollWheel:(NSEvent *)event {
    if (_renderer) {
        _renderer->onScroll((float)event.scrollingDeltaY);
    }
}

// 触控板双指捏合（与 scrollWheel 是不同的事件类型）
- (void)magnifyWithEvent:(NSEvent *)event {
    if (_renderer) {
        _renderer->onPinch((float)event.magnification);
    }
}

- (void)keyDown:(NSEvent *)event {
    NSString *characters = event.charactersIgnoringModifiers;
    if (characters.length == 1) {
        unichar c = [characters characterAtIndex:0];
        if (c >= '1' && c <= '9') {
            [self switchToExerciseAtIndex:(NSInteger)(c - '1')];
            return;
        }
    }

    if (_renderer) {
        switch (event.keyCode) {
            case 126: _renderer->onKeyZoom(true);  return; // ↑
            case 125: _renderer->onKeyZoom(false); return; // ↓
            case 123: _renderer->onKeyOrbit(false); return; // ←
            case 124: _renderer->onKeyOrbit(true);  return; // →
            default: break;
        }
    }

    [super keyDown:event];
}

#pragma mark - 尺寸变化

- (void)reshape {
    [super reshape];
    [self updateBackingSize];
}

// 主线程把视图尺寸换算成像素尺寸缓存，供渲染线程读取
- (void)updateBackingSize {
    NSRect backing = [self convertRectToBacking:self.bounds];
    _vpWidth  = (GLsizei)backing.size.width;
    _vpHeight = (GLsizei)backing.size.height;
}

- (void)dealloc {
    if (_displayLink) {
        CVDisplayLinkStop(_displayLink);
        CVDisplayLinkRelease(_displayLink);
        _displayLink = NULL;
    }
    // 在 GL 资源析构前让上下文 current，确保 glDelete* 生效
    [self.openGLContext makeCurrentContext];
    _exercise.reset();
    _renderer.reset();
}

@end

#pragma clang diagnostic pop
