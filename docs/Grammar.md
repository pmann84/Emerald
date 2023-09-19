$$
\begin{align}
[\text{prog}] &\to [\text{stmt}]^* \\
[\text{stmt}] &\to \begin{cases}
    \text{return} \; [\text{expr}]; \\
    \text{let}\space\text{identifier} = [\text{expr}];
\end{cases}\\
[\text{expr}] &\to \begin{cases}
    \text{int\_lit} \\
    \text{identifier}
\end{cases}
\end{align}
$$
