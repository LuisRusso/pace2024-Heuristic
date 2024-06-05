# pace2024-Heuristic
Submission to the heuristic track of pace 2024.

Implementation simulated annealing algorithm for the one-sided crossing
minimization problem.

## Table of contents

- [Getting Started]
   - [Prerequisites]
   - [Installing]
   - [Running]
- [Contributing]
- [Versioning]
- [Authors]
- [License]
- [Acknowledgments]

## Getting Started

To get a copy of this software download or clone the GitHub repository.

Download:

```
wget https://github.com/LuisRusso/pace2024-Heuristic
```

Clone:

```
git clone https://github.com/LuisRusso/pace2024-Heuristic
```

### Prerequisites

This package was tested with [arch linux] btw, it will most likely work
with other Linux distributions and UNIX variants. Some version of the
following components must exist in the system.

For Linux:

* C compiler, [gcc]
* [GNU Make]
* glibc
* libbsd for [arc4random]

### Installing

Execute `make` to obtain the `project` binary.

```
make
```

### Running

The binary receives a graph in the [input format] through `stdin`.

```
./project < 1.gr
```

## Authors

* **Luís M. S. Russo** - *Initial work* - [LuisRusso]

See also the list of [contributors] who participated in this project.

## License

This project is licensed under the MIT License - see
the [LICENSE file] for details

## Acknowledgments

* This software was produced for research that was funded in by national
funds through Fundação para a Ciência e Tecnologia ([FCT]) with reference
UIDB/50021/2020.

* Thanks to [PurpleBooth] for the [README-Template].
* The [grip] tool by [Joe Esposito] was very handy for producing this file.


[Getting Started]: #getting-started
[Prerequisites]: #prerequisites
[Installing]: #installing
[Running]: #running
[Authors]: #authors
[License]: #license
[Acknowledgments]: #acknowledgments

[arch linux]: https://archlinux.org/
[gcc]: https://gcc.gnu.org/
[GNU Make]: https://www.gnu.org/software/make/
[arc4random]: https://man.archlinux.org/man/extra/libbsd/arc4random.3bsd.en
[input format]: https://pacechallenge.org/2024/io/

[lmsrusso@gmail.com]: mailto:lmsrusso@gmail.com
[tags]: ../../tags
[LuisRusso]: https://github.com/LuisRusso
[LICENSE file]: ./LICENSE
[FCT]: https://www.fct.pt/
[PurpleBooth]: https://gist.github.com/PurpleBooth
[README-Template]: https://gist.github.com/PurpleBooth/109311bb0361f32d87a2
[grip]: https://github.com/joeyespo/grip
[Joe Esposito]: https://github.com/joeyespo
