// [语义错误] UninitializedVariable：变量声明后未赋值即使用
fn use_before_init() -> i32 {
    let mut x: i32;
    let mut y: i32;

    y = x;
    return y;
}
