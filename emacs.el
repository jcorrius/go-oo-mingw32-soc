(defvar my-openoffice-path-regexp
  "openoffice"
  "Regexp that matches paths into your OpenOffice.org source tree.")

(defun my-openoffice-c-hook ()
  (when (string-match my-openoffice-path-regexp buffer-file-name)
      (message "Using OpenOffice.org code settings")
      (setq tab-width 4)
      (setq tab-stop-list '(4 8 12 16 20 24 28 32 36 40 44 48 52 56 60 64 68 72
			    76 80 84 88 92 96 100 104 108 112 116 120))
      (c-set-style "stroustrup")
      (set-variable 'c-basic-offset 4)
      (set-variable 'indent-tabs-mode nil)))

(add-hook 'c-mode-common-hook 'my-openoffice-c-hook)

(add-to-list 'auto-mode-alist (cons "\\.sdi\\'" 'c++-mode))
(add-to-list 'auto-mode-alist (cons "\\.hrc\\'" 'c++-mode))
(add-to-list 'auto-mode-alist (cons "\\.src\\'" 'c++-mode))
