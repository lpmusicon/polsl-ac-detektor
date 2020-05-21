import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { SetupMqttComponent } from './setup-mqtt.component';

describe('SetupMqttComponent', () => {
  let component: SetupMqttComponent;
  let fixture: ComponentFixture<SetupMqttComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ SetupMqttComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(SetupMqttComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
