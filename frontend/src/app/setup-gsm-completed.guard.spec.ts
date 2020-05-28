import { TestBed } from '@angular/core/testing';

import { SetupGsmCompletedGuard } from './setup-gsm-completed.guard';

describe('SetupGsmCompletedGuard', () => {
  let guard: SetupGsmCompletedGuard;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    guard = TestBed.inject(SetupGsmCompletedGuard);
  });

  it('should be created', () => {
    expect(guard).toBeTruthy();
  });
});
