// [语义正确] 类型推导：let mut x; 首次赋值时确定 i32 类型
fn infer() -> i32 {
    let mut x;
    x = 10;
    x = x + 1;
    return x;
}

fn infer_from_expr() -> i32 {
    let mut y;
    y = (1 + 2) * 3;
    return y;
}
