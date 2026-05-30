// [语义正确] 控制流与返回路径：if 分支 return 后，函数体末尾仍有 return
fn abs_like(mut n: i32) -> i32 {
    if n < 0 {
        return 0 - n;
    }
    return n;
}

fn max2(mut a: i32, mut b: i32) -> i32 {
    if a > b {
        return a;
    }
    return b;
}
