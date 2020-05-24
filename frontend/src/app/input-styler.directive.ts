import { Directive, ElementRef } from '@angular/core';

@Directive({
  selector: '[appInputStyler]',
  host: {
    'class': 'input-element'
  }
})
export class InputStylerDirective {

  constructor(el: ElementRef) {
  }

}
