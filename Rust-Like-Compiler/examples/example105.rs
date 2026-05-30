// [语义错误] CannotInferType：无类型注解且从未赋值，FinalAnalyze 报错
fn cannot_infer() -> i32 {
    let mut x;
    return 0;
}
