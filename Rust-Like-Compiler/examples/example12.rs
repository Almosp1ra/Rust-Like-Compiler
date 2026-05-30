// [语义正确] 块级作用域与变量遮蔽（if / while 内可重新声明同名变量）
fn scope_shadow() -> i32 {
    let mut x: i32;
    x = 1;

    if x < 10 {
        let mut x: i32;
        x = 100;
    }

    while x < 5 {
        let mut x: i32;
        x = 0;
        x = x + 1;
    }

    return x;
}
