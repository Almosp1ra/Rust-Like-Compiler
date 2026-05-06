// 阶乘函数
fn factorial(mut n: i32) -> i32 {
    if n <= 1 { return 1; }
    return n * factorial(n - 1);
}

fn main() -> i32 {
    let mut x: i32;
    x = 10;
    let mut result: i32;
    result = factorial(5);

    // while 循环
    let mut i: i32;
    i = 0;
    while i < 5 {
        x = x + 1;
        i = i + 1;
    }

    return 0;
}