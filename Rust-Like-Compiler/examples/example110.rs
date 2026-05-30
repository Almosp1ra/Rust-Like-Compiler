// [语义错误] ArgumentCountMismatch：实参与形参个数不符
fn add(mut a: i32, mut b: i32) -> i32 {
    return a + b;
}

fn wrong_argc() -> i32 {
    let mut x: i32;
    x = add(1);
    return x;
}
