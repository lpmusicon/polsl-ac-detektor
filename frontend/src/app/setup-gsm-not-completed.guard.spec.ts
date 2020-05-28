import { TestBed } from '@angular/core/testing';

import { SetupGsmNotCompletedGuard } from './setup-gsm-not-completed.guard';

describe('SetupGsmNotCompletedGuard', () => {
  let guard: SetupGsmNotCompletedGuard;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    guard = TestBed.inject(SetupGsmNotCompletedGuard);
  });

  it('should be created', () => {
    expect(guard).toBeTruthy();
  });
});
