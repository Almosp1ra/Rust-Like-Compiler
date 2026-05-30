// [语义警告] UnreachableStatement：return 之后的语句不可达
fn unreachable() -> i32 {
    return 1;
    let mut x: i32;
    x = 2;
    return x;
}
