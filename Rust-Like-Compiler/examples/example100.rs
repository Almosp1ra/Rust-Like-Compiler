// [语义错误] UndefinedSymbol：使用未声明的变量
fn use_undefined() -> i32 {
    let mut x: i32;
    x = unknown_var;
    return x;
}
