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

    // 游戏式移动按键状态（魔兽世界布局），keyDown 置位 / keyUp 清除
    BOOL _keyW, _keyS, _keyQ, _keyE, _keyA, _keyD;
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
    if (!_renderer) {
        return;
    }
    // 触控板松手后系统还会发 1~2 秒衰减的惯性滚动事件（momentum），
    // 叠加在插值尾巴上就是松手后仍在"飘"。直接忽略，松手即停。
    if (event.momentumPhase != NSEventPhaseNone) {
        return;
    }
    _renderer->onScroll((float)event.scrollingDeltaY);
}

// 触控板双指捏合（与 scrollWheel 是不同的事件类型）
- (void)magnifyWithEvent:(NSEvent *)event {
    if (_renderer) {
        _renderer->onPinch((float)event.magnification);
    }
}

// 按键状态合成每轴 -1/0/+1 传给渲染器，update() 每帧按 dt 推进
- (void)pushMovementInput {
    if (!_renderer) {
        return;
    }
    const float strafe  = (_keyE ? 1.0f : 0.0f) - (_keyQ ? 1.0f : 0.0f);
    const float forward = (_keyW ? 1.0f : 0.0f) - (_keyS ? 1.0f : 0.0f);
    const float turn    = (_keyA ? 1.0f : 0.0f) - (_keyD ? 1.0f : 0.0f); // yaw 增加 = 左转
    _renderer->setMovementInput(strafe, forward, turn);
}

// 用 keyCode（物理键位）而非字符，切输入法/大小写不影响；返回 NO 表示不是移动键
- (BOOL)handleMovementKey:(NSEvent *)event isDown:(BOOL)down {
    switch (event.keyCode) {
        case 12: _keyQ = down; break; // Q 左平移
        case 14: _keyE = down; break; // E 右平移
        case 13: _keyW = down; break; // W 前进
        case 1:  _keyS = down; break; // S 后退
        case 0:  _keyA = down; break; // A 左转
        case 2:  _keyD = down; break; // D 右转
        default: return NO;
    }
    [self pushMovementInput];
    return YES;
}

- (void)keyDown:(NSEvent *)event {
    // 系统按住重复的 keyDown 直接吞掉：状态已置位，重复事件只会打扰
    if ([self handleMovementKey:event isDown:YES]) {
        return;
    }

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

- (void)keyUp:(NSEvent *)event {
    if ([self handleMovementKey:event isDown:NO]) {
        return;
    }
    [super keyUp:event];
}

// 失焦时清空按键状态，避免按住期间切走导致"卡键"持续移动
- (BOOL)resignFirstResponder {
    _keyW = _keyS = _keyQ = _keyE = _keyA = _keyD = NO;
    [self pushMovementInput];
    return [super resignFirstResponder];
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
