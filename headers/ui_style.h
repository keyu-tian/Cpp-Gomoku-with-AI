#ifndef UI_STYLE_H
#define UI_STYLE_H

#include <QString>

const QString CTRL_BUTTON_STYLE =
		// 操作按钮风格（扁平，圆角，黑白）
		"QPushButton{"
		"background-color:rgba(244,244,244,200);"	// 背景色（也可以设置图片）
		"border-style:outset;"				// 边框样式（inset/outset）
		"border-width:2px;"				// 边框宽度像素
		"border-radius:18px;"				// 边框圆角半径像素
		"border-color:rgba(68,68,68,220);"		// 边框颜色
		"font:bold 13px;"				// 字体，字体大小
		"color:rgba(58,58,58,230);"			// 字体颜色
		"padding:4px;"					// 填衬
		"}"
		//鼠标按下样式
		"QPushButton:pressed{"
		"background-color:rgba(42,42,42,255);"
		"border-width:4px;"				// 边框宽度像素
		"border-color:rgba(234,234,234,255);"
		"border-style:inset;"
		"color:rgba(200,200,200,255);"
		"}"
		//鼠标悬停样式
		"QPushButton:hover{"
		"background-color:rgba(48,48,48,230);"
		"border-color:rgba(250,250,250,250);"
		"color:rgba(250,250,250,250);"
		"}";

const QString BUBBLE_BUTTON_STYLE =
		// 气泡按钮风格（方块，扁平，黑白）
		"QPushButton{"
		"background-color:rgba(250,250,250,230);"	// 背景色（也可以设置图片）
		"border-style:outset;"				// 边框样式（inset/outset）
		"border-width:2px;"				// 边框宽度像素
		"border-radius:15px;"				// 边框圆角半径像素
		"border-color:rgba(68,68,68,220);"		// 边框颜色
		"padding:6px;"					// 填衬
		"}";

const QString TITLE_BUTTON_STYLE[] = {
		// 标题栏按钮风格（扁平，圆角，黑白）
		"QPushButton{"
		"background-color:rgba(68,68,68,0);"		// 背景色（也可以设置图片）
		"border-style:outset;"				// 边框样式（inset/outset）
		"border-width:2px;"				// 边框宽度像素
		"border-radius:15px;"				// 边框圆角半径像素
		"border-color:rgba(68,68,68,0);"		// 边框颜色
		"font:bold 12px;"				// 字体，字体大小
		"color:rgba(68,68,68,240);"			// 字体颜色
		"}"
		//鼠标按下样式
		"QPushButton:pressed{"
		"font:bold 9px;"
		"}",

		"QPushButton{"
		"background-color:rgba(68,68,68,0);"		// 背景色（也可以设置图片）
		"border-style:outset;"				// 边框样式（inset/outset）
		"border-width:2px;"				// 边框宽度像素
		"border-radius:15px;"				// 边框圆角半径像素
		"border-color:rgba(68,68,68,0);"		// 边框颜色
		"font:bold 23px;"				// 字体，字体大小
		"color:rgba(68,68,68,240);"			// 字体颜色
		"}"
		//鼠标按下样式
		"QPushButton:pressed{"
		"font:bold 19px;"
		"}"
		//鼠标悬停样式（无）
		};

#endif // UI_STYLE_H
