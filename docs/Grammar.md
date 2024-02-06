# Emerald Grammar
$$
\begin{align*}
\text{[WhiteSpace]} &= \text{ ``\space" | ``\textbackslash n" | ``\textbackslash t" | ``\textbackslash r" | ``\textbackslash f" | ``\textbackslash v" ;} \\
\text{[Digit]} &= \text{``0" | ``1" | ``2" | ``3" | ``4" | ``5" | ``6" | ``7" | ``8" | ``9" ;} \\
[\text{Terminator}] &= \text{ ``;"; } \\
[\text{Symbol}] &= \text{`` \{ " | `` \} " | ``(" | ``)" | ``=" | ``;" | ``-" | ``+" | ``*"; } \\
\\
[\text{Prog}] &= \text{\{[Stmt]\};} \\
\\
\text{[SingleLineComment]} &= \text{``\#'', [Any Character except newline];}\\
\text{[MultiLineComment]} &= \text{``\#*'', [Any Character including newline], ``*\#'';}\\
[\text{Comment}] &= \text{[SingleLineComment] | [MultiLineComment];}\\ 
\\
[\text{Expr}] &= [\text{Term}] \space | \space [\text{BinaryExpr}];\\
\\
[\text{BinaryExpr}] &= [\text{Expr}] * [\text{Expr}] & \text{prec = 1} \\
&| \space [\text{Expr}]\space / \space [\text{Expr}] & \text{prec = 1} \\
&| \space [\text{Expr}] + [\text{Expr}] & \text{prec = 0} \\
&| \space [\text{Expr}] - [\text{Expr}]; & \text{prec = 0} \\
\\
[\text{Term}] &= [\text{IntegerLiteral}] \\
&| \space [\text{Identifier}] \\
&| \space \text{``(''}, \text{[Expr]}, \text{``)''};\\
\\
[\text{Scope}] &= \text{ ``\{'', \{[Stmt]\}, ``\}'';} \\
\\
[\text{Stmt}] &= \text{``return'', [Expr], [Terminator]} \\ 
&| \space \text{``let'', [Identifier], ``='', [Expr], [Terminator]} \\
&| \space [\text{Identifier}], \text{``=''}, [\text{Expr}], [\text{Terminator}] \\
&| \space [\text{Scope}] \\
&| \space [\text{Comment}] \\
&| \space \text{``if''}, \text{``(''}, \text{[Expr]}, \text{``)''}, [\text{Scope}], [\text{IfPredicate}] \\
&| \space \text{``while''}, \text{``(''}, \text{[Expr]}, \text{``)''}, [\text{Scope}]; \\
\\
[\text{IfPredicate}] &= \text{``else''}, \text{``if''}, \text{``(''}, [\text{Expr}], \text{``)''}, [\text{Scope}], [\text{IfPredicate}] \\
&| \space \text{else}, [\text{Scope}] \\
&| \space \epsilon; \\
\\
% \text{for} [\text{Expr}] [\text{LoopLimiter}] \text{identifier} [\text{LoopLimiter}] [\text{Expr}] [\text{Scope}]
% [\text{LoopLimiter}] &\to \begin{cases}
%     \text{"<"} \\
%     \text{"<="} \\
% \end{cases} \\
\end{align*}
$$

