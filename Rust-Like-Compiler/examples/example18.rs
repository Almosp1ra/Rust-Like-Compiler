// [语义正确] 同一作用域内变量重复声明（允许遮蔽，符号表应保留最新绑定）
fn redeclare_ok() -> i32 {
    let mut v: i32;
    v = 1;

    let mut v: i32;
    v = 2;

    return v;
}
