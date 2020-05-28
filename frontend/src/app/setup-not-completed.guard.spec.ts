import { TestBed } from '@angular/core/testing';

import { SetupNotCompletedGuard } from './setup-not-completed.guard';

describe('SetupNotCompletedGuard', () => {
  let guard: SetupNotCompletedGuard;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    guard = TestBed.inject(SetupNotCompletedGuard);
  });

  it('should be created', () => {
    expect(guard).toBeTruthy();
  });
});
