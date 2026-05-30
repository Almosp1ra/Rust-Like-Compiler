// [语义错误] RedefinedSymbol：全局函数重名
fn dup() -> i32 {
    return 0;
}

fn dup() -> i32 {
    return 1;
}
