import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { SetupGsmComponent } from './setup-gsm.component';

describe('SetupGsmComponent', () => {
  let component: SetupGsmComponent;
  let fixture: ComponentFixture<SetupGsmComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ SetupGsmComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(SetupGsmComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
