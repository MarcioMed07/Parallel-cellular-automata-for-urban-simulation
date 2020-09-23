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