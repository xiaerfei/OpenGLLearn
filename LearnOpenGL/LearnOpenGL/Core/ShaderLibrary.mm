//
//  ShaderLibrary.mm
//  LearnOpenGL
//

#import <Foundation/Foundation.h>

#include "ShaderLibrary.hpp"
#include "Shader.hpp"

namespace {

// 读取 bundle 内 <name>.<ext> 的文本内容，找不到返回空串。
// 着色器从各自的 demo 文件夹扁平拷入 bundle，因此按文件名直接查找即可。
std::string readShaderResource(NSString *name, NSString *ext) {
    NSString *path = [[NSBundle mainBundle] pathForResource:name ofType:ext];
    if (!path) {
        return {};
    }
    NSString *content = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
    return content ? std::string(content.UTF8String) : std::string();
}

} // namespace

bool loadShader(Shader &shader, const std::string &baseName) {
    NSString *base = [NSString stringWithUTF8String:baseName.c_str()];
    const std::string vertexSrc = readShaderResource(base, @"vert");
    const std::string fragmentSrc = readShaderResource(base, @"frag");

    if (vertexSrc.empty() || fragmentSrc.empty()) {
        NSLog(@"[ShaderLibrary] 缺少着色器资源 %@.vert 或 %@.frag", base, base);
        return false;
    }
    return shader.compile(vertexSrc, fragmentSrc);
}
