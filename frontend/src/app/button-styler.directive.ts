import { Directive } from '@angular/core';

@Directive({
  selector: '[appButtonStyler]',
  host: {
    'class': 'button-element'
  }
})
export class ButtonStylerDirective {

  constructor() { }

}
