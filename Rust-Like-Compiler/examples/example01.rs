fn add(mut a: i32, mut b: i32) -> i32 {
    return a + b;
}

fn calc(mut x: i32, mut y: i32) -> i32 {
    let mut z: i32;
    z = x + y * 2 - (x / 3);
    return z;
}

fn main() -> i32 {
    // 单行注释测试
    /* 块注释测试
       多行内容
    */

    let mut a: i32;
    let mut b: i32;
    let mut c: i32;

    a = 10;
    b = 20;
    c = add(a, b);

    if c > 20 {
        c = c - 1;
    }

    while c < 100 {
        c = c + 10;
    }

    ;
    return calc(c, 5);
}