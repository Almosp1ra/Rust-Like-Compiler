// [语义错误] RedefinedSymbol：局部变量与形参同名（形参非 Var，不允许重名）
fn clash_with_param(mut x: i32) -> i32 {
    let mut x: i32;
    x = 1;
    return x;
}
