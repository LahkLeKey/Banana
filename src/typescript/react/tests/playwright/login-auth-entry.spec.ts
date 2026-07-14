import {expect, test} from '@playwright/test';

test('login gate exposes GitHub, local sign-in, sign-up, and password reset entries', async ({page}) => {
  await page.goto('/login');

  await expect(page.getByRole('button', {name: 'Continue with GitHub'}))
      .toBeVisible();
  await expect(page.getByRole('button', {name: 'Continue with Email and Password'}))
      .toBeVisible();
  await expect(page.getByRole('button', {name: 'Create Banana Account'}))
      .toBeVisible();
  await expect(page.getByRole('button', {name: 'Forgot Password?'}))
      .toBeVisible();
});
