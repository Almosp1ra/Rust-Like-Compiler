fn nested() -> i32 {
    let mut x: i32;
    x = 1;
    if x < 10 {
        while x < 5 {
            x = x + 1;
        }
    }
    return x;
}