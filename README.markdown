# schwa 0.1.0
##### developed by samuel tenka
##### most recent feature: pass-by-reference (2019-05-24)

## Introduction
Imagine programming by specifying a desired **effect** rather than a **method** to achieve that effect.
For example, from the following code 

    def check_sorted(my_array: arr int): bool
    {
        var i: int;
        i = 0;
        do {
            i+1 < len(my_array) ~>  
                if {
                    my_array[i]  > my_array[i+1] ~> return = false
                    my_array[i] <= my_array[i+1] ~> skip
                }
        }
        return = true
    }

## A Construct for Bridging Specs and Implementation
We offer an instance of **programming with holes**, wherein the programmer's job is to **sketch** a program and to
supply a specification instead of implementation for the missing pieces.  The interpreter's job is one of structured or
constrained optimization.  Programming-with-holes appears in a range of forms.
Inference frameworks, such as **Prolog** or **Gen**, invite programs built from small, clean, discrete abstractions
such as symbols or subroutines; they view optimization as inference, whence holes correspond to variables to which to
assign values, and the specification or objective is one of logical or probabilistic consistency.
By contrast, solvers such as **AMPL** or **Keras** make it easiest to build programs from large, distributed,
continuous representations such as network flows or word vectors; they make no commitment about the interpretation of
loss functions.

With our toy language **schwa**, we aim to wed the procedural style and resultant abstraction ability of Gen with the
flexibility of Keras in optimizing arbitrary holes with arbitrary objectives.  To do so, we introduce a learning
control flow structure, the `switch` statement, that learns to take one of a set of specified branches based on the
context of local variables.  More precisely, a switch maintains at every time a function from the possible values of
its **arguments** (below, the variable `len_of_buffer`) to the space of probability distributions over its **branches**
(below, there are three branches, each starting with `~>`).  This function changes during runtime in response to the
program's experience of rewards and penalties, which occur as specified by `$(...)` expressions:

    // example of the learning construct:
    switch (len_of_buffer) {
        ~> capacity = capacity + 10
        ~> capacity = capacity * 2
        ~> skip
    };

    // example of how to specify a reward: 
    if {
        ((len_of_buffer >= 0.8 * capacity)) ~> $( -2.0 ) 
        ((len_of_buffer <  0.8 * capacity) and
         (len_of_buffer >= 0.8 * capacity)) ~> $(  0.0 ) 
        ((len_of_buffer <  0.1 * capacity)) ~> $( -1.0 ) 
    }  

Schwa compiles to C, and at this level, hidden from the user, each switch is allocated a small neural network trained
using gradient descent (with Williams' REINFORCE).  I hope, just as the potent abstractions of while-loops and
recursion rose from goto's fall, that this **controlled** introduction of learning to programs can provide adaptivity 
while respecting our need to reason about and strongly constrain our program behavior.
Schwa thus emphasizes the tradition of procedural programming even while it represents **differentiable programming**.

## Getting Started

Inspect the source `toy_examples/dijkstra.schwa` and run `make dijkstra`.

Inspect the source `toy_examples/switch_example.schwa` and run `make switch_example`.

Inspect the source `toy_examples/compare.schwa` and run `make compare`.

## Reinforcement Learning Behind the Scenes


## Feedback 

Please let sam know your thoughts!
(email `colimit.edu` --- figure out where the `@` sign is).


<!--
## RELATED WORK
### Agent-Based Programming and Reinforcement Learning
### Differentiable Programming
### Probabilistic Programs
    Gen's optimizer
### Program Induction
### Unsupervised Branch Prediction

## EXAMPLES

### learn to control 

    def score(temperature: float, pressure: float, injected_fuel: float): float
    {
        var mess: float;
        mess = pollution(temperature, injected_fuel);
        if {
            mess <= 1.0 ~> return = power(temperature, pressure, injected_fuel)
            mess >= 1.0 ~> return = -10.0
        }
    } 
    def inject(temperature: float, pressure: float): float 
    {
        switch (temperature, pressure) {
            ~> return = -1.0    // air
            ~> return =  0.0 
            ~> return = +1.0    // fuel
        };
    }
    $(score(temperature, pressure, inject(temperature, pressure)))

### learn to communicate 

    switch () {
    }

### learn to code 

    switch () {
    }

## COMPILATION
### Remembering
    have fixed sized memory buffer allowing approximate REINFORCE over large timescales
### Passing Gradients
### CPU/GPU Allocation 
-->

