import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { SignalStatusComponent } from './signal-status.component';

describe('SignalStatusComponent', () => {
  let component: SignalStatusComponent;
  let fixture: ComponentFixture<SignalStatusComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ SignalStatusComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(SignalStatusComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
