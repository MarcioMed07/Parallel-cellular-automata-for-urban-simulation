# Parallel cellular automata for urban simulation
Implementação paralela de um problema do artigo [Parallel cellular automata for large-scale urban
simulation using load-balancing techniques](https://www.tandfonline.com/doi/full/10.1080/13658810903107464) para disciplina Programação paralela e distribuida de 2020.5 na UFRRJ

## Dependencias
- [GCC](gcc.gnu.org)
- [OpenMP](openmp.org)
- [Make](gnu.org/software/make) (opcional)

Se você usar o apt como gerenciador de pacotes e já tem o Make basta rodar `make install` para instalar as outras dependencias

## Instruções
Instala dependencias se voce usar o apt
```bash
make install
```

Constroe o executavel
```bash
make build
```
Executa o programa
```bash
make run
```
Limpa a pasta do executável
```bash
make clear
```

# Considerações sobre o artigo

## ideia geral
* O celular automata para a _urban simulation_ (_urban CA_) possui regras mais complexas do que comparadas com os CA clássicos (_classic CA_)
* A celula do _urban CA_ só pode ter dois valores: **non-urbanized cell** e **urbanized cell**.
* Para saber se a celula irá mudar de estado, é calculado a ¨probabilidade de transição¨ (ou também chamada de ¨potencial de transição¨). Se este valor for menor ou maior que um valor _threshold_ (T), o estado da celula mudará.
* O artigo utiliza o **mpi** para a paralelizar o CA. Para a implementação da disciplina, (provavelmente) iremos adaptar para o **OpenMP** se possível

## MPI para _parallel CA_
* O _urban CA_ é dividida em várias _threads_, rodando em diferentes computadores.
* O _urban CA_ (vou chamar de ¨matriz¨ a partir de agora) é divídida em em ¨sub-matrizes¨ de mesmo tamanho. A ligação entre as sub-matrizes, que estão em diferentes trheads (e também podem estar em diferentes computadores) é feita pelas celulas da borda da sub-matriz, chamada de ¨celulas fantasmas¨ (_ghost cells_)
* Cada _thread_ realiza duas computações (tarefas): **mandar e receber o estado das _gosth cells_** e **calcular o estado atual da celula central**

## técnicas de distribuição dos dados (_urban CA_)
* São estudadas técnicas de qual é a melhor forma de distribuir a matriz (CA) entre as threads.
* (continua...)