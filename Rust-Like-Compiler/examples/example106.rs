// [语义错误] MissingReturn：声明返回 i32 但函数体无 return
fn no_return() -> i32 {
    let mut x: i32;
    x = 1;
}
