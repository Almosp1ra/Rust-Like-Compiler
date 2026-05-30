// [语义正确] 多函数符号表：全局函数注册、跨函数调用链
fn double(mut n: i32) -> i32 {
    return n + n;
}

fn triple(mut n: i32) -> i32 {
    return double(n) + n;
}

fn sum_three(mut a: i32, mut b: i32, mut c: i32) -> i32 {
    return a + b + c;
}

fn main() -> i32 {
    let mut result: i32;
    result = triple(3);
    result = result + sum_three(1, 2, 3);
    return result;
}
