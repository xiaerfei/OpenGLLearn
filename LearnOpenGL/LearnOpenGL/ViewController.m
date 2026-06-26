//
//  ViewController.m
//  LearnOpenGL
//
//  Created by TVUM4Pro on 2026/6/26.
//

#import "ViewController.h"
#import "GLView.h"

@implementation ViewController {
    GLView *_glView;
    NSPopUpButton *_exercisePopup;
}

static const CGFloat kBarHeight = 36.0;

- (void)viewDidLoad {
    [super viewDidLoad];

    const NSRect bounds = self.view.bounds;

    // GL 视图：占据工具条下方区域（不与控件重叠，避免 NSOpenGLView 合成问题）
    _glView = [[GLView alloc] initWithFrame:NSMakeRect(0, 0, bounds.size.width,
                                                       bounds.size.height - kBarHeight)];
    _glView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [self.view addSubview:_glView];

    // 顶部工具条：Demo 标签 + 下拉切换
    NSTextField *label = [NSTextField labelWithString:@"Demo:"];
    label.frame = NSMakeRect(12, bounds.size.height - kBarHeight + 8, 44, 20);
    label.autoresizingMask = NSViewMinYMargin | NSViewMaxXMargin;
    [self.view addSubview:label];

    _exercisePopup = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(56, bounds.size.height - kBarHeight + 6, 220, 24)
                                                pullsDown:NO];
    [_exercisePopup addItemsWithTitles:[_glView exerciseNames]];
    [_exercisePopup selectItemAtIndex:0];
    _exercisePopup.autoresizingMask = NSViewMinYMargin | NSViewMaxXMargin;
    _exercisePopup.target = self;
    _exercisePopup.action = @selector(onSelectExercise:);
    [self.view addSubview:_exercisePopup];

    // 数字键切换时，回调让下拉框跟着更新
    __weak NSPopUpButton *weakPopup = _exercisePopup;
    _glView.exerciseDidChange = ^(NSInteger index) {
        [weakPopup selectItemAtIndex:index];
    };
}

- (void)onSelectExercise:(NSPopUpButton *)sender {
    [_glView switchToExerciseAtIndex:sender.indexOfSelectedItem];
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end
