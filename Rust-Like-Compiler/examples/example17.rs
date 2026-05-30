// [语义正确] while 循环累加（四元式：条件跳转、赋值、算术）
fn sum_to_n(mut n: i32) -> i32 {
    let mut i: i32;
    let mut sum: i32;

    i = 1;
    sum = 0;

    while i <= n {
        sum = sum + i;
        i = i + 1;
    }

    return sum;
}
