// [语义错误] MissingReturn：if 分支 return 但函数末尾无 return（无 else 支持时典型场景）
fn if_no_fallback() -> i32 {
    if 1 < 2 {
        return 1;
    }
}
