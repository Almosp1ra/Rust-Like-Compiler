fn cmp() -> i32 {
    let mut x: i32;
    x = 1;
    if x == 1 {
        x = x + 1;
    }
    if x != 2 {
        x = x + 1;
    }
    if x < 10 {
        x = x + 1;
    }
    if x <= 20 {
        x = x + 1;
    }
    if x > 3 {
        x = x + 1;
    }
    if x >= 4 {
        x = x + 1;
    }
    return x;
}