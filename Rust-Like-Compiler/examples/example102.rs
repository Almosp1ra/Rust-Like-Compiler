// [语义错误] RedefinedSymbol：形参重名
fn bad_param(mut x: i32, mut x: i32) -> i32 {
    return x;
}
