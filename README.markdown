# schwa

## PROGRAMMING WITH REINFORCE
### Bridging Specs and Implementation 

## RELATED WORK
### Agent-Based Programming and Reinforcement Learning
### Differentiable Programming
### Probabilistic Programs
    Gen's optimizer
### Program Induction

## EXAMPLES

### learn to control 

    def score(temperature: float, pressure: float, injected_fuel: float): float
    {
        mess: float;
        mess = pollution(temperature, injected_fuel);
        if {
                mess <= 1.0 --> return = power(temperature, pressure, injected_fuel);
            |   mess >= 1.0 --> return = -10.0;
        }
    } 
    def inject(temperature: float, pressure: float): float 
    {
        switch (temperature, pressure) {
                return = -1.0;  // air
            |   return =  0.0;  
            |   return = +1.0;  // fuel
        };
        $(score(temperature, pressure, return));
    }

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
