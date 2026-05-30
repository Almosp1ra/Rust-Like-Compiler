// [语义错误] InvalidLeftValue：函数名不能作为赋值左值
fn add(mut a: i32, mut b: i32) -> i32 {
    return a + b;
}

fn assign_to_fn() -> i32 {
    add = 1;
    return 0;
}
