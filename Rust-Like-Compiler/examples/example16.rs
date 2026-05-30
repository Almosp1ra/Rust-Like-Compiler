// [语义正确] 形参读写、局部变量赋值、表达式语句（丢弃返回值）
fn accumulate(mut base: i32, mut delta: i32) -> i32 {
    base = base + delta;
    return base;
}

fn use_params() -> i32 {
    let mut a: i32;
    let mut b: i32;

    a = 5;
    b = 10;
    accumulate(a, b);
    a = accumulate(a, b);
    return a;
}
