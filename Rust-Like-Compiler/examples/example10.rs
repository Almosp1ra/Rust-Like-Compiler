// [语义正确] void 函数声明、函数体内 return; 与符号表注册
// 注：当前语义分析器尚不允许 void 函数作为表达式语句调用（见 example43）
fn noop() {
    return;
}

fn print_val(mut n: i32) {
    let mut tmp: i32;
    tmp = n;
    return;
}

fn main() -> i32 {
    return 0;
}
