#pragma once

/*
 * 结构体，指示源代码位置
 */
struct SourceLocation
{
	size_t column;
	size_t line;

	SourceLocation(size_t column = 0, size_t line = 0);
};