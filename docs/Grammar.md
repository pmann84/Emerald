$$
\begin{align}
[\text{Prog}] &\to [\text{Stmt}]^* \\
[\text{Stmt}] &\to \begin{cases}
    \text{return} \; [\text{Expr}]; \\
    \text{let}\space\text{identifier} = [\text{Expr}]; \\
    [\text{Scope}] \\
    \text{if} ([\text{Expr}]) \space [\text{Scope}] \\
\end{cases}\\
[\text{Scope}] &\to \begin{cases}
    \{[\text{Stmt}]^*\}
\end{cases} \\
[\text{Expr}] &\to \begin{cases}
    [\text{Term}] \\
    [\text{BinExpr}]
\end{cases} \\
[\text{BinExpr}] &\to \begin{cases}
    [\text{Expr}] * [\text{Expr}] & \text{prec = 1} \\
    [\text{Expr}]\space / \space [\text{Expr}] & \text{prec = 1} \\
    [\text{Expr}] + [\text{Expr}] & \text{prec = 0} \\
    [\text{Expr}] - [\text{Expr}] & \text{prec = 0} \\
\end{cases} \\
[\text{Term}] &\to \begin{cases}
    \text{int\_lit} \\
    \text{identifier} \\
    ([\text{Expr}])
\end{cases} \\
\end{align}
$$
