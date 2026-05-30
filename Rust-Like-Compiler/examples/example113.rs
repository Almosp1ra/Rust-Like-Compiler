// [语义错误] VoidFunctionUsedAsExpression：void 函数出现在表达式中
fn do_work(mut n: i32) {
    let mut tmp: i32;
    tmp = n;
    return;
}

fn use_void_as_expr() -> i32 {
    let mut x: i32;
    x = do_work(1);
    return x;
}
