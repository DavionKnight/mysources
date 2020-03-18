let g:winManagerWindowLayout='FileExplorer|TagList'

"copy
nmap ,v "+p
vmap ,c "+y
nmap ,c "+yy

nmap wm :WMToggle<cr>

inoremap <C-h> <Left>
inoremap <C-j> <Down>
inoremap <C-k> <Up>
inoremap <C-l> <Right>

"ctags update
function! UpdateCtags()
    let curdir=getcwd()
    while !filereadable("./tags")
        cd ..
        if getcwd() == "/"
            break
        endif
    endwhile
    if filewritable("./tags")
        !ctags -R --file-scope=yes --langmap=c:+.h --languages=c,c++ --links=yes --c-kinds=+p --c++-kinds=+p --fields=+iaS --extra=+q
        TlistUpdate
    endif
    execute ":cd " . curdir
endfunction
nmap <C-F8> :call UpdateCtags()<CR>

"add by kevin 
"if 0
autocmd BufNewFile *.c,*.h,*.sh exec "call AddTitle()"
function AddTitle()
if &filetype=='sh' 
call append( 0, "\#!/bin/bash")
call append( 1, "#")
call append( 2, "#  COPYRIGHT NOTICE")
call append( 3, "#  Copyright (C) 2016 Baidu Corporation, Inc. All rights reserved")
call append( 4, "#")
call append( 5, "#  Author             :zhangjiajie@baidu.com")
call append( 6, "#  File Name          :".expand("%:t"))
call append( 7, "#  Create Date        :".strftime("%Y/%m/%d %H:%M"))
call append( 8, "#  Last Modified      :".strftime("%Y/%m/%d %H:%M"))
call append( 9, "#  Description        :NA")
call append(10, "#")
call append(11, "")
else
call append( 0, "/*")
call append( 1, "*  COPYRIGHT NOTICE")
call append( 2, "*  Copyright (C) 2016 Baidu Corporation, Inc. All rights reserved")
call append( 3, "*")
call append( 4, "*  Author             :zhangjiajie@baidu.com")
call append( 5, "*  File Name          :".expand("%:t"))
call append( 6, "*  Create Date        :".strftime("%Y/%m/%d %H:%M"))
call append( 7, "*  Last Modified      :".strftime("%Y/%m/%d %H:%M"))
call append( 8, "*  Description        :NA")
call append( 9, "*/")
call append(10, "")
endif
echohl WarningMsg | echo "Successful in adding the copyright." | echohl None
endfunction


function SetLastModifiedTimes()
	let line = getline(8)
	let newtime = "*  Last Modified      :".strftime("%Y/%m/%d %H:%M")
	let repl = substitute(line,".*$",newtime,"g")
	let res = search("*  Last Modified","w")
	if res
		call setline(8,repl)
	endif
endfunction
autocmd BufWrite *.c call SetLastModifiedTimes()

if has("cscope")

    """"""""""""" Standard cscope/vim boilerplate

    " use both cscope and ctag for 'ctrl-]', ':ta', and 'vim -t'
    set cscopetag

    " check cscope for definition of a symbol before checking ctags: set to 1
    " if you want the reverse search order.
    set csto=0

    " add any cscope database in current directory
    if filereadable("cscope.out")
        cs add cscope.out  
    " else add the database pointed to by environment variable 
    elseif $CSCOPE_DB != ""
        cs add $CSCOPE_DB
    endif

    " show msg when any other cscope db added
    set cscopeverbose  


    """"""""""""" My cscope/vim key mappings
    "
    " The following maps all invoke one of the following cscope search types:
    "
    "   's'   symbol: find all references to the token under cursor
    "   'g'   global: find global definition(s) of the token under cursor
    "   'c'   calls:  find all calls to the function name under cursor
    "   't'   text:   find all instances of the text under cursor
    "   'e'   egrep:  egrep search for the word under cursor
    "   'f'   file:   open the filename under cursor
    "   'i'   includes: find files that include the filename under cursor
    "   'd'   called: find functions that function under cursor calls
    "
    " Below are three sets of the maps: one set that just jumps to your
    " search result, one that splits the existing vim window horizontally and
    " diplays your search result in the new window, and one that does the same
    " thing, but does a vertical split instead (vim 6 only).
    "
    " I've used CTRL-\ and CTRL-@ as the starting keys for these maps, as it's
    " unlikely that you need their default mappings (CTRL-\'s default use is
    " as part of CTRL-\ CTRL-N typemap, which basically just does the same
    " thing as hitting 'escape': CTRL-@ doesn't seem to have any default use).
    " If you don't like using 'CTRL-@' or CTRL-\, , you can change some or all
    " of these maps to use other keys.  One likely candidate is 'CTRL-_'
    " (which also maps to CTRL-/, which is easier to type).  By default it is
    " used to switch between Hebrew and English keyboard mode.
    "
    " All of the maps involving the <cfile> macro use '^<cfile>$': this is so
    " that searches over '#include <time.h>" return only references to
    " 'time.h', and not 'sys/time.h', etc. (by default cscope will return all
    " files that contain 'time.h' as part of their name).


    " To do the first type of search, hit 'CTRL-\', followed by one of the
    " cscope search types above (s,g,c,t,e,f,i,d).  The result of your cscope
    " search will be displayed in the current window.  You can use CTRL-T to
    " go back to where you were before the search.  
    "

    nmap <C-\>s :cs find s <C-R>=expand("<cword>")<CR><CR>	
    nmap <C-\>g :cs find g <C-R>=expand("<cword>")<CR><CR>	
    nmap <C-\>c :cs find c <C-R>=expand("<cword>")<CR><CR>	
    nmap <C-\>t :cs find t <C-R>=expand("<cword>")<CR><CR>	
    nmap <C-\>e :cs find e <C-R>=expand("<cword>")<CR><CR>	
    nmap <C-\>f :cs find f <C-R>=expand("<cfile>")<CR><CR>	
    nmap <C-\>i :cs find i ^<C-R>=expand("<cfile>")<CR>$<CR>
    nmap <C-\>d :cs find d <C-R>=expand("<cword>")<CR><CR>	


    " Using 'CTRL-spacebar' (intepreted as CTRL-@ by vim) then a search type
    " makes the vim window split horizontally, with search result displayed in
    " the new window.
    "
    " (Note: earlier versions of vim may not have the :scs command, but it
    " can be simulated roughly via:
    "    nmap <C-@>s <C-W><C-S> :cs find s <C-R>=expand("<cword>")<CR><CR>	

    nmap <C-@>s :scs find s <C-R>=expand("<cword>")<CR><CR>	
    nmap <C-@>g :scs find g <C-R>=expand("<cword>")<CR><CR>	
    nmap <C-@>c :scs find c <C-R>=expand("<cword>")<CR><CR>	
    nmap <C-@>t :scs find t <C-R>=expand("<cword>")<CR><CR>	
    nmap <C-@>e :scs find e <C-R>=expand("<cword>")<CR><CR>	
    nmap <C-@>f :scs find f <C-R>=expand("<cfile>")<CR><CR>	
    nmap <C-@>i :scs find i ^<C-R>=expand("<cfile>")<CR>$<CR>	
    nmap <C-@>d :scs find d <C-R>=expand("<cword>")<CR><CR>	


    " Hitting CTRL-space *twice* before the search type does a vertical 
    " split instead of a horizontal one (vim 6 and up only)
    "
    " (Note: you may wish to put a 'set splitright' in your .vimrc
    " if you prefer the new window on the right instead of the left

    nmap <C-@><C-@>s :vert scs find s <C-R>=expand("<cword>")<CR><CR>
    nmap <C-@><C-@>g :vert scs find g <C-R>=expand("<cword>")<CR><CR>
    nmap <C-@><C-@>c :vert scs find c <C-R>=expand("<cword>")<CR><CR>
    nmap <C-@><C-@>t :vert scs find t <C-R>=expand("<cword>")<CR><CR>
    nmap <C-@><C-@>e :vert scs find e <C-R>=expand("<cword>")<CR><CR>
    nmap <C-@><C-@>f :vert scs find f <C-R>=expand("<cfile>")<CR><CR>	
    nmap <C-@><C-@>i :vert scs find i ^<C-R>=expand("<cfile>")<CR>$<CR>	
    nmap <C-@><C-@>d :vert scs find d <C-R>=expand("<cword>")<CR><CR>


    """"""""""""" key map timeouts
    "
    " By default Vim will only wait 1 second for each keystroke in a mapping.
    " You may find that too short with the above typemaps.  If so, you should
    " either turn off mapping timeouts via 'notimeout'.
    "
    "set notimeout 
    "
    " Or, you can keep timeouts, by uncommenting the timeoutlen line below,
    " with your own personal favorite value (in milliseconds):
    "
    "set timeoutlen=4000
    "
    " Either way, since mapping timeout settings by default also set the
    " timeouts for multicharacter 'keys codes' (like <F1>), you should also
    " set ttimeout and ttimeoutlen: otherwise, you will experience strange
    " delays as vim waits for a keystroke after you hit ESC (it will be
    " waiting to see if the ESC is actually part of a key code like <F1>).
    "
    "set ttimeout 
    "
    " personally, I find a tenth of a second to work well for key code
    " timeouts. If you experience problems and have a slow terminal or network
    " connection, set it higher.  If you don't set ttimeoutlen, the value for
    " timeoutlent (default: 1000 = 1 second, which is sluggish) is used.
    "
    "set ttimeoutlen=100

endif

"add for taglist by kevin 2016-11-16
" 按F8按钮，在窗口的左侧出现taglist的窗口,像vc的左侧的workpace
set mouse=n 
nnoremap <silent> <C-l> :TlistToggle<CR><CR>
" :Tlist              调用TagList
let Tlist_Exit_OnlyWindow=1                  " 如果Taglist窗口是最后一个窗口则退出Vim
let Tlist_Use_Left_Window=1                 " left窗口中显示
let Tlist_File_Fold_Auto_Close=1             " 自动折叠
let Tlist_Auto_Open=1
let Tlist_Process_File_Always=1

set fencs=utf-8,gbk

set fileformats=unix,dos

"a table key=4bytes by kevin 2017-04-14
set ts=4
set expandtab
set shiftwidth=4

"autoindent
set autoindent
set cindent
set smartindent
set vb 				"screen flash
set scrolloff=8	"screen move by scroll
set softtabstop=4	"one backspace delete 4 space
set wrap			"long line auto wrap


set ruler
set showmatch
set incsearch
set hls

syntax enable
syntax on
set background=dark
colorscheme desert
set nu

set nocompatible
set backspace=indent,eol,start
"高亮颜色设置，先在任一文件中执行：hi 得到合适的颜色 2018-7-4
hi Search term=reverse cterm=bold ctermbg=1 gui=bold guibg=Red

"endif

