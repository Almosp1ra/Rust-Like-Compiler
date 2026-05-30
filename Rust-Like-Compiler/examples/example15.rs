// [语义正确] 复杂表达式与嵌套调用（适合后续四元式生成测试）
fn mul(mut a: i32, mut b: i32) -> i32 {
    return a * b;
}

fn add(mut a: i32, mut b: i32) -> i32 {
    return a + b;
}

fn complex_expr() -> i32 {
    let mut x: i32;
    let mut y: i32;
    let mut z: i32;

    x = 1 + 2 * 3 - 4 / 2;
    y = (x + 1) * (x - 1);
    z = add(mul(x, 2), mul(y, 3));
    z = add(z, (add(1, 2) + mul(3, 4)));
    return z;
}
