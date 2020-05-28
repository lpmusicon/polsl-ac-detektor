import { TestBed } from '@angular/core/testing';

import { SetupCompletedGuard } from './setup-completed.guard';

describe('SetupCompletedGuard', () => {
  let guard: SetupCompletedGuard;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    guard = TestBed.inject(SetupCompletedGuard);
  });

  it('should be created', () => {
    expect(guard).toBeTruthy();
  });
});
