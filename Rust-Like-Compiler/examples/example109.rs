// [语义错误] FunctionNotFound：调用不存在的函数
fn call_missing() -> i32 {
    let mut x: i32;
    x = not_exist(1, 2);
    return x;
}
