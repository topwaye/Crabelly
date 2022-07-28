# Crabelly
The Crabelly Operating System Project

Back to old time, I had a big dream to build a computer operating system. I was too young to understand how I could carry out something so complicated. The dream was still there, but I was not. It seemed the dream would never ever come true, until I got an inspiration from math. I love math, very good at it.

The kernel is a mathematical model which is called Trapezoid Set. Trapezoid Set is the root logic behind the kernel, which is exactly what the kernel looks like.

Crabs, nasty things, but delicious. We call this new OS Crabelly.

Crabelly is based on the Unix Operating System project at Carnegie-Mellon University.

Crabelly runs on both uniprocessor and multiprocessor computers connected by networks, ranging from smart phones to personal home systems. Crabelly is designed to provide advanced kernel facilities, in particular, virtual memory management and network communication.

Copy-on-write or simply COW is a good theory which fools a lot of people. It is supposed to create a duplicate process which shares the virtual memory pages with its parent process. If any of these processes try to modify the shared pages then a copy of these pages will be created. Eventually the duplicate process will have its own copy of the parent process. Sounds good, but that is a lie, because fork() system call does not work this way in Unix. Then what happens ? 

The data resident in an anonymous page is volatile, therefore splitting anonymous pages violates the rule of never manipulating volatile variables, which means splitting anonymous maps will never happen, that is, copy-on-write is restricted.

Another question: Do a share map and a copy-on-write map coexist at the same layer ? Absolutely not. If they do, a bug follows, meaning a map can only be shared the split part, not the unsplit part, in this undesirable situation.

What is the prototype of an OS ? Of course, it is the switchboard of a telephone system used by a company, etc., where telephone calls are answered and put through(= connected) to the appropriate person or department.

An OS which has nothing to do with Linux is destined to do nothing with the real world. That is the dull side. The bright side is that this OS can teach you what an excellent OS is, what you can do with the kernel as you want, and what theory to base upon.

When reading the Linux source code, you can hardly tell where you are. There are only a small number of people who might know the entire kernel code in the Linux core team. Linux is a kernel that you just cannot read through. That is a good news, which means no man is an expert, each is a part of the main, which means we are all equal at this point, no man could be a hero who takes charge of everything in the Linux world. We use it, but we do not touch it.

How to separate the physical map layer from the machine-independent code of the Linux virtual memory system ? Goodbye Linux.

Designing, implementing, and testing an operating system as large and complex as Unix is quite a challenge. That is how I built this OS. Make the unknown known.

I would like to thank those who take the time to learn about my work and provide me with valuable feedback on how it could be improved.

I would also like to take this chance to say to those who dream big in computer science: Go for it.

topwaye@hotmail.com
