fn f(mut x: i32) -> i32 {
    return x + 1;
}

fn g() -> i32 {
    let mut y: i32;
    y = f(10);
    y = f((1 + 2) * 3);
    return y;
}