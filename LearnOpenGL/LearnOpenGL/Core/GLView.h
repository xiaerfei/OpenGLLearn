//
//  GLView.h
//  LearnOpenGL
//
//  请求 OpenGL 4.1 Core Profile 上下文的 NSOpenGLView，自带 CVDisplayLink 渲染循环。
//  头文件保持纯 Objective-C，可被 .m 文件安全 import。
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

// NSOpenGLView 在 macOS 10.14 起弃用但仍是学习 OpenGL 的原生路径，静默告警。
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

@interface GLView : NSOpenGLView

// 已注册的练习名称（顺序与数字键 1..N 一致），供 UI 构建切换控件。
- (NSArray<NSString *> *)exerciseNames;

// 切换到指定下标的练习（线程安全：内部会锁定 GL 上下文）。
- (void)switchToExerciseAtIndex:(NSInteger)index;

// 练习发生切换后回调（含数字键触发），用于让外部 UI 同步选中项。主线程调用。
@property (nonatomic, copy, nullable) void (^exerciseDidChange)(NSInteger index);

@end

#pragma clang diagnostic pop

NS_ASSUME_NONNULL_END
