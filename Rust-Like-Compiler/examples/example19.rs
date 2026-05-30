// [语义正确] 综合样例：多函数 + 控制流 + 调用 + 类型推导（端到端语义通过）
fn inc(mut n: i32) -> i32 {
    return n + 1;
}

fn fib_step(mut a: i32, mut b: i32) -> i32 {
    return a + b;
}

fn main() -> i32 {
    let mut a;
    let mut b;
    let mut i: i32;
    let mut tmp: i32;

    a = 0;
    b = 1;
    i = 0;

    while i < 5 {
        tmp = fib_step(a, b);
        a = b;
        b = tmp;
        i = inc(i);
    }

    if b > 10 {
        return b;
    }

    return a + b;
}
